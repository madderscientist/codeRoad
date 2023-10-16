const config = require('../../config.json');

const fs = require('fs');
const os = require('os');
const Path = require('path');
const { cpu } = require('node-os-utils');
const express = require('express');
const iconv = require('iconv-lite');
const child_process = require('child_process');

const DFA = require('../utils/dfa');


// paper管理模块
const paper_dfa_config = {
    "$name": "paper",
    "$init_state": "ready",
    "$log": true,
    "$": {
        paper_subproc: null,
        auto_restart: config.paper.auto_restart,
        to_restart: false,
    },
    "ready": {
        "$comment": "就绪，等待启动",
        "$pre_triggerr": ($) => {
            if ($.to_restart) {
                $.to_restart = false;
                return 'running';
            } else return '$nop';
        },
        "@start": "running",
    },
    "running": {
        "$comment": "正在运行",
        "$pre_trigger": ($, dsa) => {
            // start child-process paper
            let proc = child_process.spawn(config.paper.script_java, config.paper.script_params, {
                cwd: config.paper.path,
            });
            proc.stdout.on('data', (data) => process_output(data, 1));
            proc.stderr.on('data', (data) => process_output(data, 2));
            proc.on('close', () => dsa.emit("closed"));

            $.paper_subproc = proc;
        },
        "@closed": ($) => $.auto_restart ? "relaunching" : "ready",
        "@stop": "closing",
    },
    "closing": {
        "$comment": "正在通过sigint关闭",
        "$pre_trigger": ($) => {
            $.paper_subproc.kill('SIGINT');
        },
        "@closed": "ready",
        "$timeout_ms": config.paper.sigint_timeout,
        "@$timeout": "aborting",
    },
    "aborting": {
        "$comment": "正在通过sigkill关闭",
        "$pre_trigger": ($) => {
            $.paper_subproc.kill('SIGKILL');
        },
        "@closed": "ready",
    },
    "relaunching": {
        "$comment": "等待自动重启",
        "$timeout_ms": config.paper.restart_delay,
        "@$timeout": "running",
    },
}

let paper = new DFA(paper_dfa_config);


let router_paper = express.Router();

router_paper.get("/status", (req, res) => {
    (async () => {
        const cpuUsage = await cpu.usage().catch(() => 0);
        const totalMemory = os.totalmem();
        const freeMemory = os.freemem();

        res.json({
            "err": "",
            "server_cpu": cpuUsage,
            "server_mem_used": (totalMemory - freeMemory),
            "server_mem_total": totalMemory,
            "paper_status": paper.get_state(),
            "paper_player_list": "Unknown",
            "paper_ping": 0.0
        });
    })();
});

const PaperLogFile = Path.join(config.paper.path, config.paper.log);
router_paper.get("/log", (req, res) => {
    fs.readFile(PaperLogFile, (err, data) => {
        if (err) res.json({ "err": err.message });
        else res.json({ "err": "", "log": iconv.decode(data, "utf8") });
    });
});


const stdio_encoding = config.paper.stdio_encoding;

// process stdout/stderr from child-proc
let on_reply = null;
function process_output(org_chunk, id = 0) {
    let str = iconv.decode(org_chunk, stdio_encoding);

    let color;
    if (id === 1) color = 32;
    else if (id === 2) color = 35;
    else color = 33;
    process.stdout.write(`\x1B[${color}m${str}\x1B[0m`);

    let chunk = iconv.encode(str, 'utf8');

    if (on_reply === null) return;
    if (!on_reply.writable) {
        on_reply = null;
        return;
    }

    on_reply.write(chunk);
    // if (id === 1) {
    //     // stdout
    //     on_reply.write(chunk);
    // } else if (id === 2) {
    //     // stderr
    //     on_reply.write(chunk);
    // } else {
    //     // system
    //     on_reply.write(chunk);
    // }
}

// send chunk to child-proc
function process_input(chunk, callback) {
    paper.get_$().paper_subproc.stdin.write(chunk, callback);
}

router_paper.get("/log_stream", (req, res) => {
    process_output("[System]: Connection reset.\n");
    on_reply = res;
    process_output("[System]: Connected.\n");
});

router_paper.post("/stdin", (req, res) => {
    let chunk = iconv.encode(req.body['msg'], stdio_encoding);
    process_input(chunk, (error) => res.json({ err: error ? error.message : "" }));
});

router_paper.post("/status", (req, res) => {
    let msg = req.body['msg'];

    if (msg === 'nop') {
        // nop
    } else if (msg === 'start') {
        paper.emit('start');
    } else if (msg === 'stop') {
        paper.get_$().to_restart = false;
        paper.emit('stop');
    } else if (msg === 'restart') {
        paper.get_$().to_restart = true;
        paper.emit('stop');
    } else {
        res.json({err:`Unknown operator ${msg}`})
        return;
    } 
    res.json({err: ''});    
});

module.exports = router_paper;
