// Route Setup
const express = require('express');

const router = express.Router();

router.get(
  '/profile',
  (req, res, next) => {
    res.json({
      user: req.user,
      token: req.query.secret_token
    });
  }
);

router.get(
  '/deviceList',
  (req, res, next) => {
    //res.send(particleAPISession.deviceList);
  }
)

module.exports = router;
