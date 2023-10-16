const config = require('../../config.json');
const express = require('express');

let router_api = express.Router();

router_api.use('', require('./files'));
router_api.use('', require('./paper'));

module.exports = router_api;
