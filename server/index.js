const express = require('express')
const app = express()

app.set('view engine', 'ejs');

app.get('/', (req, res) => {
	res.render('index');
});

app.post('/data', (req, res) => {

});

app.listen(3000, () => console.log('Server running on port 3000'))

