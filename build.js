const fs = require('fs');
const os = require('os');
const path = require('path');


// Read a text file, normalizing line endings.
function readTextFileSync(pth) {
  var data = fs.readFileSync(pth, 'utf8');
  return data.replace(/\r?\n|\r/g, os.EOL);
}


// Write a text file, normalizing line endings.
function writeTextFileSync(pth, text) {
  var data = text.replace(/\r?\n|\r/g, os.EOL);
  fs.writeFileSync(pth, data, 'utf8');
}


// Call a function for each file in a directory tree.
function forEachFileRecSync(rootDir, fn) {
  var entries = fs.readdirSync(rootDir, {withFileTypes: true});
  for (var entry of entries) {
    var pth = path.join(rootDir, entry.name);
    if (entry.isDirectory()) forEachFileRecSync(pth, fn);
    else if (entry.isFile()) fn(pth);
  }
}


// Change all #include directives to use relative paths.
function changeIncludesToRelative(rootDir) {
  forEachFileRecSync(rootDir, function(pth) {
    var text = readTextFileSync(pth);
    var dir  = path.dirname(pth);
    text = text.replace(/#include\s+["<]catch2\/(.*)[">]/g, function(match, inc) {
      var absInc = path.resolve(rootDir, inc);
      var relInc = path.relative(dir, absInc);
      return '#include "' + relInc.replace(/\\/g, '/') + '"  // Adjust to relative path (@wolfram77)';
    });
    writeTextFileSync(pth, text);
  });
}


// Amagamate all .cpp files into corresponding .hpp files.
function amagamateCppsIntoHpps(rootDir) {
  forEachFileRecSync(rootDir, function(pth) {
    if (!pth.endsWith('.cpp')) return;
    var cppPath = pth;
    var hppPath = cppPath.replace(/\.cpp$/, '.hpp');
    if (!fs.existsSync(hppPath)) return;
    var cppText = readTextFileSync(cppPath);
    var hppText = readTextFileSync(hppPath);
    cppText = cppText.replace(/#include\s+["<](.*)[">]/g, function(match, inc) {
      return path.basename(inc) === path.basename(hppPath)? `// ${match} // Disable self-include (@wolfram77)` : match;
    });
    incGuard = path.basename(hppPath.replace(/\..*/, '')).toUpperCase() + '_CPP_INCLUDED';
    hppText  = hppText.trim() + `\n\n\n\n` +
      `// BEGIN Amalgamated content from ${path.basename(cppPath)} (@wolfram77)\n` +
      `#ifndef ${incGuard}\n` +
      `#define ${incGuard}\n` +
      `#ifdef CATCH2_IMPLEMENTATION\n` +
      cppText.trim() + `\n` +
      `#endif // CATCH2_IMPLEMENTATION\n` +
      `#endif // ${incGuard}\n` +
      `// END Amalgamated content from ${path.basename(cppPath)} (@wolfram77)\n`;
    writeTextFileSync(hppPath, hppText);
    fs.unlinkSync(cppPath);
  });
}


// Perform the build steps.
changeIncludesToRelative('catch2');
amagamateCppsIntoHpps('catch2');
