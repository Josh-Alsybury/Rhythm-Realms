const http = require('http');
const url = require('url');
const https = require('https');
const querystring = require('querystring');

const CLIENT_ID = '28e081af3dfc453296f07c23ffe6d88f';
const CLIENT_SECRET = '4cae7109583c4f728ad63f6262133403';
const REDIRECT_URI = 'http://127.0.0.1:8888/callback';

let tokens = {}; // simple in-memory token storage
let lastTrackId = null;
let currentBPM = 120;

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

    res.on('data', (chunk) => {
      body += chunk;
    });

    res.on('end', () => {
      try {
        const parsed = JSON.parse(body);
        if (parsed.error) {
          console.error('Error fetching tokens:', parsed);
        } else {
          tokens.access_token = parsed.access_token;
          tokens.refresh_token = parsed.refresh_token;
          console.log('Tokens received and stored:', tokens);
        }
      } catch (err) {
        console.error('Error parsing token response', err);
      }
    });
  });

  req.on('error', (e) => {
    console.error('Request error:', e);
  });

  req.write(postData);
  req.end();
}

// Fetch currently playing track
// Fetch currently playing track
function getCurrentlyPlaying(callback) {
  if (!tokens.access_token) {
    console.log('❌ No access token available');
    callback({ error: 'No access token yet.' });
    return;
  }

  console.log('📡 Polling Spotify API...');

  const options = {
    hostname: 'api.spotify.com',
    path: '/v1/me/player/currently-playing',
    method: 'GET',
    headers: {
      'Authorization': `Bearer ${tokens.access_token}`,
    },
  };

const req = https.request(options, (res) => {
  console.log(`🔍 Spotify API Response Status: ${res.statusCode}`);

  if (res.statusCode === 401) {
    refreshAccessToken();
    callback({ error: 'Token refreshing' });
    return;
  }

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

      // ✅ LOCAL TRACK CHECK — CORRECT LOCATION
      if (parsed?.item?.is_local) {
        console.log('⚠ Local track — BPM unavailable');
        callback({
          playing: parsed?.is_playing ?? false,
          bpm: currentBPM
        });
        return;
      }

      const trackId = parsed?.item?.id;
      const trackName = parsed?.item?.name;
      const artistName = parsed?.item?.artists?.[0]?.name;

      if (trackId && trackId !== lastTrackId) {
        lastTrackId = trackId;
        fetchTrackBPM(trackId);
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

  req.on('error', (e) => {
    console.error('❌ Request error:', e.message);
    callback({ error: e.message, bpm: currentBPM });
  });
  
  req.end();
}

function fetchTrackBPM(trackId) {
  console.log(`🎼 Fetching BPM for track: ${trackId}`);

  const options = {
    hostname: 'api.spotify.com',
    path: `/v1/audio-features/${trackId}`,
    method: 'GET',
    headers: {
      'Authorization': `Bearer ${tokens.access_token}`,
    },
  };

  const req = https.request(options, (res) => {
    console.log(`🔍 Audio Features Status: ${res.statusCode}`);

    if (res.statusCode === 403) {
      console.log('🚫 BPM unavailable for this track (Spotify restriction)');
      return;
    }

    let data = '';
    res.on('data', (chunk) => data += chunk);
    res.on('end', () => {
      try {
        const parsed = JSON.parse(data);
        if (parsed.tempo) {
          currentBPM = parsed.tempo;
          console.log(`✅ BPM updated: ${currentBPM}`);
        }
      } catch (err) {
        console.error('❌ BPM parse error:', err);
      }
    });
  });

  req.on('error', (e) => console.error('❌ BPM request error:', e));
  req.end();
}

function refreshAccessToken() {
  if (!tokens.refresh_token) return;

  const postData = querystring.stringify({
    grant_type: 'refresh_token',
    refresh_token: tokens.refresh_token,
  });

  const authString = Buffer.from(`${CLIENT_ID}:${CLIENT_SECRET}`).toString('base64');

  const options = {
    hostname: 'accounts.spotify.com',
    path: '/api/token',
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
      'Authorization': `Basic ${authString}`,
      'Content-Length': Buffer.byteLength(postData),
    },
  };

  const req = https.request(options, (res) => {
    let body = '';
    res.on('data', (chunk) => body += chunk);
    res.on('end', () => {
      const parsed = JSON.parse(body);
      if (parsed.access_token) {
        tokens.access_token = parsed.access_token;
        console.log('🔄 Access token refreshed');
      }
    });
  });

  req.write(postData);
  req.end();
}

const { exec } = require('child_process');

function openLogin() {
  const scopes = 'user-read-playback-state user-read-currently-playing'
  const authURL =
    'https://accounts.spotify.com/authorize?' +
    querystring.stringify({
      response_type: 'code',
      client_id: CLIENT_ID,
      scope: scopes,
      redirect_uri: REDIRECT_URI
    });

  exec(`start "" "${authURL}"`);
}



// Basic HTTP server
const server = http.createServer((req, res) => {
  const parsedUrl = url.parse(req.url, true);

  // handle OAuth callback
  if (parsedUrl.pathname === '/callback') {
    const code = parsedUrl.query.code;
    console.log('Incoming path:', parsedUrl.pathname);// this simple log statement fixed the damn issue of nothing appearing at http://127.0.0.1:8888/current
    if (code) {
      exchangeCodeForTokens(code);
      res.writeHead(200, { 'Content-Type': 'text/plain' });
      res.end('Authorization successful! Tokens being fetched...');
    } else {
      res.writeHead(400, { 'Content-Type': 'text/plain' });
      res.end('No authorization code found.');
    }

  // handle "currently playing" route
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
  console.log('Server running at http://127.0.0.1:8888/');
  openLogin();
});
