const path      = require('path');
const express   = require('express');
const exphbs    = require('express-handlebars');
const bodyParsr = require('body-parser');
const routes    = require('./routes.js');
const messenger = require('./messenger.js');
const port      = 25565;
const app       = express();

app.use(bodyParsr.json())
app.use(bodyParsr.urlencoded({ extended: false }))
app.use(express.static(__dirname + '/static'));
app.engine('.hbs', exphbs({  
  defaultLayout: 'main',
  extname: '.hbs',
  layoutsDir: path.join(__dirname, 'views/layouts')
}));
app.set('view engine', '.hbs');
app.set('views', path.join(__dirname, 'views'));
routes(app, messenger);

app.listen(port, function(err) {
  if (err) {
    return console.log('something bad happened', err);
  }
  console.log('server is listening on ' + port);
});