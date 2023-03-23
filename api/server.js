const express = require('express');
const mysql = require('mysql');
const cors = require ('cors');
const app = express();
const bodyParser = require('body-parser')
const port = process.env.PORT || 8080


app.listen(port, () => console.log('Listen on port ' + port))
//Mysql

app.use(function(req, res, next){
    res.locals.connection = mysql.createConnection({
    connectionLimit : 10,
    host : 'localhost',
    user : 'root',
    password : '',
    database : 'CO2GAZ'
});
    res.locals.connection.connect();
    next();
});

const corsOptions ={
    origin:'*', 
    method : 'GET,PUT,POST,DELETE,OPTIONS',
    credentials:true,            //access-control-allow-credentials:true
    optionSuccessStatus:200
}
app.use(cors(corsOptions));

// JSON body parser, there is no `extended` option here
//app.use(express.json());

// parse application/x-www-form-urlencoded
app.use(bodyParser.urlencoded({ extended: false }))

// parse application/json
app.use(bodyParser.json())

//API pour gérer les ACTIVITES

app.get('/api/controller/historiqueCO2', function(req, res, next){ 
    res.locals.connection.query('Select * from historiqueCO2', function(error, results, fields){
        if (error) throw error;
        res.json(results);
    })
});

app.get('/api/controller/historiqueGAZ', function(req, res, next){ 
    res.locals.connection.query('Select * from historiqueGAZ', function(error, results, fields){
        if (error) throw error;
        res.json(results);
    })
});

// const express = require("express");
// const app = express();
// const admin = require("firebase-admin");
// const serviceAccount = require("./key.json");

// admin.initializeApp({
//   credential: admin.credential.cert(serviceAccount),

// });

// const db = admin.database();

// app.use(express.json());

// app.user(express.urlencoded({extended : true}));

// const port = process.env.PORT || 8080

// app.listen(port, () => console.log('Listen on port ' + port))


// app.post('/create', async (req, res) => {
//     try {
//         console.log(req.body);
//         const id = req.body.id;
//         const historiqueJson = {
//             niveau : req.body.niveau,
//             date : new Date()    
//         }
//         const response = db.collection("historiqueCO2").doc(id).set(historiqueJson);
//         res.send(response);
//     } catch(error) {
//         res.send(error);
//     }
// })