// DotENV setup
require('dotenv').config();
const secret = process.env.API_SECRET;
// Route Setup
const express = require('express');
const passport = require('passport');
const jwt = require('jsonwebtoken');

const router = express.Router();

router.post(
  '/signup',
  passport.authenticate('signup', {session: false}),
  async (req, res, next) => {
    res.json({
      success: true,
      user: req.user,
    });
  }
);

router.post(
  '/login',
  async (req, res, next) => {
    passport.authenticate(
      'login',
      async (err, user, info) => {
        try {
          if (err) {
            const error = new Error(info.message);
            return next(error);
          }

          if (!user) {
            return res.json({
              success: false,
              message: info.message,
            });
          }

          req.login(
            user,
            {session: false},
            async (error) => {
              if(error) return next(error);

              const body = { _id: user._id, username: user.username };
              const token = jwt.sign({user: body}, secret);

              return res.json({
                success: true,
                token,
              });
            }
          );
        } catch (err) {
          return next(err);
        }
      }
    )(req, res, next);
  }
);

module.exports = router;
