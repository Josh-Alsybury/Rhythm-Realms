const http = require('http');
const url = require('url');
const https = require('https');
const querystring = require('querystring');
const fs = require('fs');

const CLIENT_ID = '28e081af3dfc453296f07c23ffe6d88f';
const CLIENT_SECRET = '4cae7109583c4f728ad63f6262133403';
const REDIRECT_URI = 'http://127.0.0.1:8888/callback';
const RAPIDAPI_KEY = 'f2d68e0833msh84f1270c3c1c05ap1a9a24jsnc36f89e6cb90';

let tokens = {};
let lastTrackId = null;
let currentBPM = 120;

// Load cached BPMs from file (if exists)
let bpmCache = {};
try {
  bpmCache = JSON.parse(fs.readFileSync('bpm_cache.json', 'utf8'));
  console.log(`📦 Loaded ${Object.keys(bpmCache).length} cached BPMs from previous sessions`);
} catch (err) {
  console.log('📦 Starting with empty BPM cache');
}

// Save cache to file
function saveBPMCache() {
  fs.writeFileSync('bpm_cache.json', JSON.stringify(bpmCache, null, 2));
}

// Exchange authorization code for tokens
function exchangeCodeForTokens(code) {
  const postData = querystring.stringify({
    grant_type: 'authorization_code',
    code: code,
    redirect_uri: REDIRECT_URI,
  });

  const authString = Buffer.from(`${CLIENT_ID}:${CLIENT_SECRET}`).toString('base64');

  const options = {
    hostname: 'accounts.spotify.com',
    path: '/api/token',
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
      'Content-Length': Buffer.byteLength(postData),
      'Authorization': `Basic ${authString}`,
    },
  };

  const req = https.request(options, (res) => {
    let body = '';
    res.on('data', (chunk) => { body += chunk; });
    res.on('end', () => {
      try {
        const parsed = JSON.parse(body);
        if (parsed.error) {
          console.error('Error fetching tokens:', parsed);
        } else {
          tokens.access_token = parsed.access_token;
          tokens.refresh_token = parsed.refresh_token;
          console.log('✅ Tokens received');
        }
      } catch (err) {
        console.error('Error parsing token response', err);
      }
    });
  });

  req.on('error', (e) => console.error('Request error:', e));
  req.write(postData);
  req.end();
}

// Fetch currently playing track
function getCurrentlyPlaying(callback) {
  if (!tokens.access_token) {
    callback({ error: 'No access token yet.', bpm: currentBPM });
    return;
  }

  const options = {
    hostname: 'api.spotify.com',
    path: '/v1/me/player/currently-playing',
    method: 'GET',
    headers: { 'Authorization': `Bearer ${tokens.access_token}` },
  };

  const req = https.request(options, (res) => {
    if (res.statusCode === 204) {
      callback({ playing: false, bpm: currentBPM });
      return;
    }

    let data = '';
    res.on('data', (chunk) => data += chunk);
    res.on('end', () => {
      if (!data) {
        callback({ playing: false, bpm: currentBPM });
        return;
      }

      try {
        const parsed = JSON.parse(data);
        const trackId = parsed?.item?.id;
        const trackName = parsed?.item?.name;
        const artistName = parsed?.item?.artists?.[0]?.name;

        // If track changed, check cache first, then fetch if needed
        if (trackId && trackId !== lastTrackId) {
          lastTrackId = trackId;
          console.log(`🔄 Track changed: ${trackName} by ${artistName}`);
          
          // Check cache first!
          if (bpmCache[trackId]) {
            currentBPM = bpmCache[trackId];
            console.log(`📦 Using cached BPM: ${currentBPM} (saved 1 API call!)`);
          } else {
            fetchBPMFromRapidAPI(trackId);
          }
        }

        callback({
          track: trackName,
          artist: artistName,
          bpm: currentBPM,
          playing: parsed?.is_playing
        });
      } catch (err) {
        callback({ error: err.message, bpm: currentBPM });
      }
    });
  });

  req.on('error', (e) => callback({ error: e.message, bpm: currentBPM }));
  req.end();
}

// Fetch BPM from RapidAPI using Spotify track ID
function fetchBPMFromRapidAPI(spotifyTrackId) {
  console.log(`🎼 Fetching BPM from RapidAPI for track: ${spotifyTrackId}`);

  const options = {
    hostname: 'track-analysis.p.rapidapi.com',
    path: `/pktx/spotify/${spotifyTrackId}`,
    method: 'GET',
    headers: {
      'x-rapidapi-key': RAPIDAPI_KEY,
      'x-rapidapi-host': 'track-analysis.p.rapidapi.com'
    }
  };

  const req = https.request(options, (res) => {
    console.log(`📊 RapidAPI Response Status: ${res.statusCode}`);

    let data = '';
    res.on('data', (chunk) => data += chunk);
    res.on('end', () => {
      try {
        const parsed = JSON.parse(data);
        
        if (res.statusCode === 429) {
          console.log('⚠ Rate limit hit - keeping last BPM:', currentBPM);
          return;
        }
        
        if (parsed.tempo) {
          currentBPM = parsed.tempo;
          bpmCache[spotifyTrackId] = currentBPM;  // Cache it!
          saveBPMCache();  // Save to file for next time
          console.log(`✅ BPM updated: ${currentBPM} (cached for future use)`);
        } else {
          console.log('⚠ No tempo in response:', parsed);
        }
      } catch (err) {
        console.error('❌ Error parsing RapidAPI response:', err);
      }
    });
  });

  req.on('error', (e) => console.error('❌ RapidAPI request error:', e));
  req.end();
}

const { exec } = require('child_process');

function openLogin() {
  const scopes = 'user-read-playback-state user-read-currently-playing';
  const authURL = 'https://accounts.spotify.com/authorize?' +
    querystring.stringify({
      response_type: 'code',
      client_id: CLIENT_ID,
      scope: scopes,
      redirect_uri: REDIRECT_URI
    });

  exec(`start "" "${authURL}"`);
}

// HTTP server
const server = http.createServer((req, res) => {
  const parsedUrl = url.parse(req.url, true);

  if (parsedUrl.pathname === '/callback') {
    const code = parsedUrl.query.code;
    if (code) {
      exchangeCodeForTokens(code);
      res.writeHead(200, { 'Content-Type': 'text/plain' });
      res.end('✅ Authorization successful!');
    } else {
      res.writeHead(400, { 'Content-Type': 'text/plain' });
      res.end('❌ No authorization code');
    }

  } else if (parsedUrl.pathname === '/current') {
    getCurrentlyPlaying((data) => {
      res.writeHead(200, { 'Content-Type': 'application/json' });
      res.end(JSON.stringify(data, null, 2));
    });

  } else {
    res.writeHead(404, { 'Content-Type': 'text/plain' });
    res.end('Not Found');
  }
});

server.listen(8888, '127.0.0.1', () => {
  console.log('🎵 Server running at http://127.0.0.1:8888/');
  openLogin();
});