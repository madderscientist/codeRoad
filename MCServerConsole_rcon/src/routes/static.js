const config = require('../../config.json');
const express = require('express');

let router_static = express.Router();

router_static.use("/static", express.static("./static"));
router_static.get("/", (req, res) => res.redirect("/hrcon/static/index.html"));

module.exports = router_static;
