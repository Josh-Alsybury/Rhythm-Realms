const http = require('http');
const url = require('url');
const https = require('https');
const querystring = require('querystring');

const CLIENT_ID = '28e081af3dfc453296f07c23ffe6d88f';
const CLIENT_SECRET = '4cae7109583c4f728ad63f6262133403';
const REDIRECT_URI = 'http://127.0.0.1:8888/callback';

let tokens = {}; // simple in-memory token storage

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
function getCurrentlyPlaying(callback) {
  if (!tokens.access_token) {
    callback({ error: 'No access token yet.' });
    return;
  }

  const options = {
    hostname: 'api.spotify.com',
    path: '/v1/me/player/currently-playing',
    method: 'GET',
    headers: {
      'Authorization': `Bearer ${tokens.access_token}`,
    },
  };

  const req = https.request(options, (res) => {
    let data = '';
    res.on('data', (chunk) => data += chunk);
    res.on('end', () => {
      try {
        const parsed = JSON.parse(data);
        callback(parsed);
      } catch {
        callback({ error: 'No track playing or bad response' });
      }
    });
  });

  req.on('error', (e) => callback({ error: e.message }));
  req.end();
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
});
