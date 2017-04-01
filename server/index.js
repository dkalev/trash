const express = require('express')
const app = express()

app.set('view engine', 'ejs');

app.get('/', (req, res) => {
	localStorage.setItem("dean", "bean");
	res.render('index');
});

app.post('/data', (req, res) => {
	localStorage.setItem("dean", "bean");
});

app.listen(3000, () => console.log('Server running on port 3000'))

