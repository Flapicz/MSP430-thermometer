const config = require('config');
const mongoose = require('mongoose');
const dbURI = config.get('dbURI');


const connect2Db = async () =>{
    try{
        await mongoose.connect(dbURI);
        console.log("Connected to DB.");
    }
    catch(error){
        throw error;
    }
}

const onDisconnectListener = ()=>{
    mongoose.connection.on('disconnect', ()=>{
        console.log('Disconnected from DB')
    })
}

module.exports = {connect2Db, onDisconnectListener};
