const crypto = require('crypto');
const fs = require('fs');
const https = require('https');
const os = require('os');
const path = require('path');
const process = require('process');
const child_process = require('child_process');
const unzipper = require('unzipper');

// The platform packages themselves and the packageName mappings are from the CodeLLDB project
// CodeLLDB is distributed under the MIT License.
// https://github.com/vadimcn/vscode-lldb
// The MIT License (MIT)

// Copyright (c) 2016 Vadim Chugunov

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
const platforms = {
  'x64-linux': { packageName: 'codelldb-x86_64-linux.vsix', hash: 'c8bfc7ea8e26861213402b5cdf0ee1c57a31c1955fe26d5ae936cfad9b2b1a09' },
  // Not supported yet: "arm64-linux": "codelldb-aarch64-linux.vsix",
  //Not supported yet: "arm-linux": "codelldb-arm-linux.vsix",
  'x64-darwin': { packageName: 'codelldb-x86_64-darwin.vsix', hash: '9e2ed9db382b4c60410a762df061f7bab4a0b6ded694261bd0e2412fc6d9306a' },
  //Not supported yet: "arm64-darwin": "codelldb-aarch64-darwin.vsix",
  'x64-win32': { packageName: 'codelldb-x86_64-windows.vsix', hash: '67871446426e177776c643fa19d29e9f287c8493731b6296a262d585f356b44b' },
  //Not supported yet: "ia32-win32": "codelldb-x86_64-windows.vsix",
};

const zigDownloads = {
  'x64-linux': {
    packageName: 'zig-linux-x86_64-0.10.0-dev.1888+e3cbea934',
    hash: 'ead05e35cc58f93e8ddf6a0feca3f5dfaa5a232d175edee88ae6ef96fa0bfada',
  },
  'x64-darwin': {
    packageName: 'zig-macos-x86_64-0.10.0-dev.1888+e3cbea934',
    hash: 'a897bf8809d4ef95385580437eb7e37dfbf7f1dbb36adb599e24bc566ca8d49d',
  },
};

function downloadFile(url, file) {
  return new Promise((resolve, reject) => {
    function handleDownload(res) {
      if (res.statusCode !== 200) {
        console.error(`Failed to download LLDB platform file (status code: ${res.statusCode})`);
        process.exit(3);
      }
      res.pipe(fs.createWriteStream(file)).on('finish', () => {
        resolve();
      });
    }
    const req = https.get(url, res => {
      if (res.statusCode === 302) {
        https.get(res.headers.location, res => {
          handleDownload(res);
        });
      } else {
        handleDownload(res);
      }
    });
  });
}
function hashFile(file) {
  return new Promise((resolve, reject) => {
    const hasher = crypto.createHash('sha256');
    const stream = fs.createReadStream(file);
    stream.on('error', err => reject(err));
    stream.on('data', buf => {
      hasher.update(buf);
    });
    stream.on('close', () => {
      resolve(hasher.digest('hex'));
    });
  });
}

// Note: this function is not safe to use with unknown zips
// Among other things, it hasn't been scrutinized for vulnerabilities like directory traversal
async function unzipFile(file, destDir) {
  return new Promise((resolve, reject) => {
    fs.createReadStream(file)
      .pipe(unzipper.Parse())
      .on('entry', entry => {
        if (entry.type !== 'File') {
          entry.autodrain();
          return;
        }
        if (!entry.path.startsWith('extension/lldb')) {
          entry.autodrain();
          return;
        }
        const filePath = entry.path.substring('extension/'.length);
        if (filePath.includes('..') || filePath.includes('\0')) {
          console.error(`File filed name requirement in unzipFile: ${filePath}`);
          process.exit(5);
        }
        // The path isn't actually guaranteed to use forward slashes, but the CodeLLDB files do
        const parts = filePath.split('/');
        for (const part of parts) {
          if (!/^[A-z0-9.-]+$/.test(part)) {
            console.error(`File failed name requirement in unzipFile: ${filePath}`);
            process.exit(6);
          }
        }
        const outDir = path.join(destDir, ...parts.slice(0, parts.length - 1));
        if (!outDir.startsWith(destDir)) {
          console.error(`File name issue: ${filePath}`);
          process.exit(7);
        }
        const outFile = path.join(destDir, filePath);
        if (!outFile.startsWith(destDir)) {
          console.error(`File name issue: ${filePath}`);
          process.exit(8);
        }
        fs.promises.mkdir(outDir, { recursive: true }).then(() => {
          entry.pipe(fs.createWriteStream(outFile)).on('finish', () => {
            fs.chmod(outFile, 0o755, err => {
              if (err) {
                console.error(`Error changing file permissions for lldb executable`, err);
                process.exit(9);
              }
            });
          });
        });
      })
      .on('close', () => {
        resolve();
      });
  });
}
async function compareHash(file, expectedHash) {
  const actualHash = await hashFile(file);
  if (expectedHash !== actualHash) {
    console.error(`Hash mismatch. Expected: ${expectedHash} Actual: ${actualHash}`);
    process.exit(4);
  }
}
async function deleteDirectoryIfPresent(dir) {
  try {
    await fs.promises.rm(dir, { recursive: true });
  } catch (e) {
    if (e.code !== 'ENOENT' && e.code !== 'ENOTDIR') {
      throw e;
    }
  }
}
async function run() {
  const platformId = `${os.arch()}-${os.platform()}`;
  if (!platforms[platformId]) {
    console.error(`Unknown platform ("${platformId}"). Add it to .system/download_tools.js`);
    process.exit(1);
  }
  const { packageName, hash: expectedHash } = platforms[platformId];
  const url = `https://github.com/vadimcn/vscode-lldb/releases/download/v1.7.0/${packageName}`;

  console.log(`Package url is ${url}`);

  const file = path.join(os.tmpdir(), 'vizcon-codelldb-package.zip');
  console.log('Downloading file...');
  await downloadFile(url, file);
  console.log('Download complete.');
  await compareHash(file, expectedHash);
  console.log('Removing old files');
  // Clear old packages
  await deleteDirectoryIfPresent('platform/lldb');
  await deleteDirectoryIfPresent('platform/mingw64');
  await deleteDirectoryIfPresent('platform/zig');

  console.log('Extracting package...');
  await unzipFile(file, 'platform');
  console.log('Finished extracting files.');
  await removeFile(file);
  console.log('Deleted lldb zip file.');

  if (platformId === 'x64-win32') {
    await downloadMingw();
  } else {
    await downloadZig(platformId);
  }
}

function removeFile(file) {
  return new Promise((resolve, reject) => {
    fs.unlink(file, err => {
      if (err) {
        reject(err);
      } else {
        resolve();
      }
    });
  });
}
function runCommand(cmd) {
  return new Promise((resolve, reject) => {
    const proc = child_process.exec(cmd, (err, stdout, stderr) => {
      if (err) {
        reject(err);
      }
      console.log(stdout);
      console.log(stderr);
      resolve(proc.exitCode);
    });
  });
}
async function downloadZig(platformId) {
  if (!zigDownloads[platformId]) {
    console.error(`Unknown platform ("${platformId}"). Add it to .system/download_tools.js`);
    process.exit(1);
  }
  const { packageName, hash: expectedHash } = zigDownloads[platformId];
  const url = `https://ziglang.org/builds/${packageName}.tar.xz`;
  console.log(`Downloading zig from ${url}`);
  await downloadFile(url, 'zig.tar.xz');
  console.log('Zig download complete. Comparing hash');
  await compareHash('zig.tar.xz', expectedHash);
  console.log('Extracting zig');
  const res = await runCommand(`tar -xf zig.tar.xz --directory platform`);
  console.log(`Extraction finished. Exit code ${res}`);
  fs.promises.rename(`platform/${packageName}`, 'platform/zig');
  console.log('Deleting zig.tar.xz');
  await removeFile('zig.tar.xz');
  console.log('Deleted zig.tar.xz');
}

async function downloadMingw() {
  console.log('Downloading mingw-w64');
  await downloadFile(
    'https://github.com/RyanG10/mingw-w64/releases/download/x86_64-8.1.0-release-posix-sjlj-rt_v6-rev0.7z/mingw64.exe',
    'mingw64.exe'
  );
  console.log('Mingw download complete');
  const mingwHash = '67a7e4a7f08218893f5d5e9ca395bb13ec52e7f4beecc19a19a08114472299d0';
  await compareHash('mingw64.exe', mingwHash);
  await new Promise(resolve => {
    child_process.exec(`start /w mingw64.exe -o"platform" -y`, (err, stdout, stderr) => {
      if (err && err.code !== 0) {
        resolve({ out: stdout, err: stderr });
        return;
      }
      resolve({ out: stdout, err: stderr });
    });
  });
  console.log('Finished extracting mingw64.');
  // Sometimes Windows will fail to delete the file. Add a delay in case that helps.
  await delay(2000);
  console.log('Deleting mingw64.exe...');
  await removeFile('mingw64.exe');
  console.log('Deleted mingw64.exe');
}

function delay(millis) {
  return new Promise(resolve => {
    setTimeout(() => {
      resolve();
    }, millis);
  });
}

run();
