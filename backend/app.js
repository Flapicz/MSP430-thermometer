const express = require('express');
const app = express();
const db = require('./db.js')
const PORT = process.env.PORT || 20917 || $PORT
const cors = require('cors')
const measurementModel = require('./measurementModel.js')

app.use(cors({ origin: true }));
app.use(express.json());

db.connect2Db()
db.onDisconnectListener()
app.listen(PORT, ()=>{
    console.log(`server listening on port ${PORT}`)
})

app.get('/', async (req,res)=>{
    const measurementDocs = await measurementModel.find().sort({createdAt: -1});
    res.status(200).json(measurementDocs);
})

app.get('/delete-all', async (req,res)=>{
    await measurementModel.deleteMany();
	res.status(200).send('All documents has been deleted');
    
})


app.post('/', async (req, res) => {
    const newMeasurement = new measurementModel(req.body)
    try{
      const savedDoc = await newMeasurement.save();
      res.status(200).json(savedDoc)
    }catch(error){
      console.log(error)
    }
  })