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
      return '#include "' + relInc.replace(/\\/g, '/') + '"';
    });
    writeTextFileSync(pth, text);
  });
}
changeIncludesToRelative('catch2');
