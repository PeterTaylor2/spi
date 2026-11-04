import {createServer} from 'node:http';
import {readFile} from 'node:fs/promises';
import {fileURLToPath} from 'node:url';
import {resolve, sep, extname} from 'node:path';
import {chromium} from 'playwright';

const root = fileURLToPath(new URL('.', import.meta.url));
const types = {'.mjs': 'text/javascript', '.wasm': 'application/wasm', '.html': 'text/html'};
const server = createServer(async (request, response) => {
    try {
        const path = resolve(root, '.' + new URL(request.url, 'http://localhost').pathname);
        if (!path.startsWith(root.endsWith(sep) ? root : root + sep)) throw new Error('Invalid path');
        const body = await readFile(path);
        response.writeHead(200, {'Content-Type': types[extname(path)] || 'application/octet-stream'});
        response.end(body);
    } catch {
        response.writeHead(404);
        response.end();
    }
});
await new Promise(resolve => server.listen(0, '127.0.0.1', resolve));
let browser;
try {
    browser = await chromium.launch({headless: true});
    const page = await browser.newPage();
    const errors = [];
    page.on('pageerror', error => { errors.push(String(error)); console.error(error); });
    await page.goto(`http://127.0.0.1:${server.address().port}/test.html`);
    await page.waitForFunction(() => window.testResult?.done, null, {timeout: 60000});
    const result = await page.evaluate(() => window.testResult);
    console.log(JSON.stringify(result, null, 2));
    if (result.error || errors.length) throw new Error(result.error || errors.join('\n'));
} finally {
    if (browser) await browser.close();
    await new Promise(resolve => server.close(resolve));
}
