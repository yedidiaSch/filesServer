const express = require('express');
const router = express.Router();
const fileController = require('../controllers/fileController');
const multer = require('multer');
const path = require('path');

// תיקיית יעד לשמירה זמנית של הקבצים
const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        cb(null, path.join(__dirname, '../uploads/'));
    },
    filename: (req, file, cb) => {
        cb(null, file.originalname); // שומר את השם המקורי
    }
});

const upload = multer({ storage });


// שימוש ב-upload.single כ-middleare לפני הפונקציה של הקונטרולר
router.post('/upload', upload.single('file'), fileController.uploadFile);
router.delete('/file/:filename', fileController.deleteFile);

module.exports = router;
