const express = require('express');
const path = require('path');
const cookieParser = require('cookie-parser');
const logger = require('morgan');
const database = require('./config/database.config');
const cors = require('cors')

const apiRouter = require('./routes/index.router')
const app = express();

database.connect();

app.use(logger('dev'));
app.use(cors())

app.use(express.json({limit: "50mb"}));
app.use(express.urlencoded({ limit: "50mb",extended: false }));
app.use(cookieParser());

app.use(express.static(path.join(__dirname, 'public')));

app.use('/mascotas', apiRouter);

app.use((error,req,res,next)=>{
    console.error(error);
    return res.status(500).json({error: "Internal Server Error"});
})

module.exports = app;
