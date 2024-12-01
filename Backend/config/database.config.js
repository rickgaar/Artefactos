const mongoose =  require("mongoose");
const debug = require("debug")('myapi:database');

const dbhost = process.env.DBHOST || 'localhost';
const dbname = process.env.DBNAME || 'mascotas';
const dbport = process.env.DBPORT || '27017';

const dbURI = process.env.DBURI || `mongodb://${dbhost}:${dbport}/${dbname}`;

//connect to database

const connect = async () => {
    try {
        await mongoose.connect(dbURI);
        debug('Connection to database started');
       
    } catch (error) {
        console.error(error);
        debug('Cannot connect to database')
        process.exit(1);
    }
}

//disconnect from database

const disconnect = async () =>{
    try {

        await mongoose.disconnect();
        debug('Connection to database ended');

    } catch (error) {
        process.exit(1);
    }
}

module.exports = {
    connect,
    disconnect
}