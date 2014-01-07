
var fs = require('fs');
var path = require('path');
var assert = require('assert');
var encode = require('./encode');

var findVolume = function (startPath, startStat) {

  var lastDev = startStat.dev;
  var lastIno = startStat.ino;
  var lastPath = startPath;

  while (1) {

    var parentPath = path.resolve(lastPath, '..');
    var parentStat = fs.statSync(parentPath);

    if (parentStat.dev !== lastDev) {
      return lastPath;
    }

    if (parentStat.ino === lastIno) {
      return lastPath;
    }

    lastDev = parentStat.dev;
    lastIno = parentStat.ino;
    lastPath = parentPath;

  }
};

module.exports = exports = function (targetPath) {

  var info = { version: 2, extra: [] };

  var parentPath = path.resolve(targetPath, '..');
  var targetStat = fs.statSync(targetPath);
  var parentStat = fs.statSync(parentPath);
  var volumePath = findVolume(targetPath, targetStat);
  var volumeStat = fs.statSync(volumePath);

  assert(targetStat.isFile() || targetStat.isDirectory());

  info.target = {
    id: targetStat.ino,
    type: (targetStat.isDirectory() ? 'directory' : 'file'),
    filename: path.basename(targetPath),
    created: targetStat.ctime
  };

  info.parent = {
    id: parentStat.ino,
    name: path.basename(parentPath)
  };

  info.volume = {
    name: path.basename(volumePath),
    created: volumeStat.ctime,
    signature: 'H+',
    type: (volumePath === '/' ? 'local' : 'other')
  };

  info.extra.push({
    type: 0,
    data: new Buffer(info.parent.name, 'utf8')
  });

  info.extra[0].length = info.extra[0].data.length;

  info.extra.push({
    type: 1,
    length: 4,
    data: new Buffer(4)
  });

  info.extra[1].data.writeUInt32BE(info.parent.id, 0);

  return encode(info);
};
