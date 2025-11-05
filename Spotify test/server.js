const http = require('http');
const url = require('url');
const https = require('https');
const querystring = require('querystring');

const CLIENT_ID = '28e081af3dfc453296f07c23ffe6d88f';
const CLIENT_SECRET = '4cae7109583c4f728ad63f6262133403';
const REDIRECT_URI = 'http://127.0.0.1:8888/callback';

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
        const tokens = JSON.parse(body);
        if (tokens.error) {
          console.error('Error fetching tokens:', tokens);
        } else {
          console.log('Tokens received:', tokens);
          // TODO: Store tokens safely here for your game to use (access_token, refresh_token)
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

const server = http.createServer((req, res) => {
  if (req.url.startsWith('/callback')) {
    const query = url.parse(req.url, true).query;
    const code = query.code;
    const state = query.state;

    if (code) {
      exchangeCodeForTokens(code);
      res.writeHead(200, { 'Content-Type': 'text/plain' });
      res.end(`Authorization code received:\n${code}\nState:\n${state}\nExchanging code for tokens...`);
    } else {
      res.writeHead(400, { 'Content-Type': 'text/plain' });
      res.end('No code found in the callback.');
    }
  } else {
    res.writeHead(404);
    res.end();
  }
});

server.listen(8888, '127.0.0.1', () => {
  console.log('Server running at http://127.0.0.1:8888/');
});
