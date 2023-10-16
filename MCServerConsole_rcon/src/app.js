const config = require('../config.json');
const express = require('express');

const app = express();

const bodyParser = require('body-parser');
const json_parser = bodyParser.json();
app.use(json_parser);

app.use('/hrcon', require('./routes/static'))
app.use('/hrcon/api', require('./routes/api'))

module.exports = app;