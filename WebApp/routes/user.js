// Route Setup
const express = require('express');
// API
const api = require('../db/api');

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
  async (req, res, next) => {
    const deviceList = await api.getDeviceList(null);
    res.json(deviceList);
  }
);

module.exports = router;
