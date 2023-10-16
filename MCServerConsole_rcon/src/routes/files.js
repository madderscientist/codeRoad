const config = require('../../config.json');
const express = require('express');
const iconv = require('iconv-lite');
const fs = require('fs');
const Path = require('path');
const DataStore = require('nedb');
const util = require('util');

let router_files = express.Router();

const root_dir = config.files.exports;
const max_size = config.files.max_size;

function mid_check_path(req, res, next) {
    let rel_path = req.query['path'];
    if (!rel_path) {
        res.send({ err: `Query 'path' not found.` });
        return;
    }

    let fullpath = Path.join(root_dir, rel_path);
    if (Path.relative(root_dir, fullpath).startsWith('..')) {
        res.send({ err: `Path '${fullpath}' not valid.` });
        return;
    }
    if (!fs.existsSync(fullpath)) {
        res.send({ err: `Path '${fullpath}' not found.` });
        return;
    }

    next();
}

router_files.get('/folder', mid_check_path, (req, res) => {
    let rel_path = req.query['path'];
    let fullpath = Path.join(root_dir, rel_path);

    if (!fs.statSync(fullpath).isDirectory()) {
        res.send({ err: `Path '${fullpath}' is not directory.` });
        return;
    }

    res.send({
        err: "",
        content: fs.readdirSync(fullpath, { withFileTypes: true }).map(file =>
            file.isDirectory() ? file.name + '/' : file.name),
    });
})

router_files.all('/file', mid_check_path, (req, res) => {
    let rel_path = req.query['path'];
    let fullpath = Path.join(root_dir, rel_path);

    if (!fs.statSync(fullpath).isFile()) {
        res.send({ err: `Path '${fullpath}' is not file.` });
        return;
    }
    if (fs.statSync(fullpath).size > max_size) {
        res.send({ err: `#file_too_large` });
        return;
    }

    let encoding = req.query['encoding'];
    if (!encoding) encoding = 'utf8';
    if (!iconv.encodingExists(encoding)) {
        res.send({ err: `Encoding '${encoding}' not supported.` });
        return;
    }

    if (req.method == "GET") {
        fs.readFile(fullpath, (err, data) => {
            if (err) res.send({ err: `Err: ${err.message}` });
            else res.send({ err: "", data: iconv.decode(data, encoding) });
        })
    } else if (req.method == "POST") {
        let towrite = iconv.encode(req.body.data, encoding);
        if (towrite.length > max_size) {
            res.send({ err: `#file_too_large` });
            return;
        }

        fs.writeFile(fullpath, towrite, {}, (err) => {
            res.send({ err: err ? err.message : "" });
        });
    } else res.sendStatus(405);
})

let history_count = 0;
let db;

async function prepare() {
    fs.mkdirSync(config.files.history_save, { recursive: true });

    let db_file = Path.join(config.files.history_save, 'history');
    let new_db = !fs.existsSync(db_file);
    let db = new Datastore({ filename: db_file });
    if (new_db) db.insert({ id: 0 });

    history_count =  db.c
}

function delete_all_history() {

}

function replace_file(path,) {

}

module.exports = router_files;
