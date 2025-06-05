class FileController {
    uploadFile(req, res) {
    console.log("Received POST /upload");
    if (!req.file) {
        console.log("No file received.");
        return res.status(400).json({ message: 'No file uploaded.' });
    }
    console.log("File received:", req.file.originalname);
    res.status(200).json({ message: 'File uploaded successfully.', file: req.file });
}


    deleteFile(req, res) {
        const filename = req.params.filename;
        // Logic to delete the file from the server would go here
        res.status(200).json({ message: `File ${filename} deleted successfully.` });
    }
}

module.exports = new FileController();