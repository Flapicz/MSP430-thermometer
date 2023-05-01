const mongoose = require('mongoose')

const measurementSchema = mongoose.Schema({
    tempValue: {
        type: String,
        required: true
    },
}, {timestamps: true})

module.exports = measurementModel = mongoose.model('measurementModel', measurementSchema)