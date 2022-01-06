// Route Setup
const express = require('express');
// Api
const api = require('../db/api');
const particle = require('../db/particle');

const router = express.Router();

router.get(
  '/:deviceId/ticks',
  (req, res) => {
    const { deviceId } = req.params;
    const { to, from, resolution } = req.query;
    const toDate = to ? new Date(Number(to)) : undefined;
    const fromDate = from ? new Date(Number(from)) : undefined;
    const setResolution = resolution ? Number(resolution) : undefined;
    api.getLastTick(deviceId, toDate, fromDate, setResolution).then( (data) => {
        res.json(data);
    })
    .catch(err => {console.log(err)});
  }
);

router.get(
  '/:deviceId/currentState',
  async (req, res) => {
    const state = await api.getCurrentState(req.params.deviceId);
    res.json(state);
  }
);

router.get(
  '/:deviceId/pumpState',
  (req, res) => {
    api.getPumpStates(req.params.deviceId).then( (data) => {
        res.json(data);
    })
    .catch(err => {console.log(err)});;
  }
);

router.get(
  '/:deviceId/status',
  async (req, res) => {
    const status = await api.getDeviceStatus(req.params.deviceId);
    res.json(status);
  }
);

router.get(
  '/:deviceId/configuration',
  (req, res) => {
    api.getConfiguration(req.params.deviceId).then( (data) => {
        res.json([data]); // Front-end expects an array
    })
    .catch(err => {console.log(err)});;
  }
);

router.post(
  '/:deviceId/configuration',
  (req, res) => {
    api.setConfiguration(req.params.deviceId, req.body);
    try {
      particle.sendConfiguration(req.params.deviceId, req.body);
      res.json({
        success: true,
        configuration: req.body,
      });
    } catch(err) {
      res.json({ success: false });
    }
  }
);

router.get(
  '/:deviceId/logs',
  async (req, res) => {
    const logs = await api.getLog(req.params.deviceId, req.body);
    res.json(logs);
  }
);

router.delete(
  '/:deviceId/logs/:logId',
  async (req, res) => {
    await api.clearLog(req.params.deviceId, req.params.logId);
    res.send();
  }
);

router.get(
  '/:deviceId/flush',
  async (req, res) => {
    const flushStates = await api.getSystemStates(req.params.deviceId, ['FLUSH']);
    res.json(flushStates);
  }
);

router.get(
  '/:deviceId/restart',
  async (req, res) => {
    const restarts = await api.getRestarts(req.params.deviceId);
    res.json(restarts);
  }
);

router.get(
  '/:deviceId/states',
  async (req, res) => {
    const states = await api.getSystemStates(req.params.deviceId, ['IDLE', 'FILL', 'FLUSH']);
    res.json(states);
  }
);

module.exports = router;