const express = require('express');
const multer = require('multer');
const errorHandler = require('./middleware/errorHandler');
const fileRoutes = require('./routes/fileRoutes');
require('dotenv').config();

const app = express();
const PORT = process.env.PORT || 3000;

const upload = multer({ dest: 'uploads/' });

app.use(express.json());
app.use(express.urlencoded({ extended: true }));

app.use('/api/files', fileRoutes);

app.use(errorHandler);

app.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
});