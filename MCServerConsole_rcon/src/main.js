const config = require('../config.json');
const app = require('./app')

app.get("/", (req, res) => res.redirect("/hrcon"));

app.listen(config.http.port, () => {
    console.log(`Rcon-http started on http://localhost:${config.http.port}`);
})
