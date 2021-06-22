// DotENV setup
require('dotenv').config();
const secret = process.env.API_SECRET;
// Passport setup
const passport = require('passport');
const LocalStrategy = require('passport-local').Strategy;
const JwtStrategy = require('passport-jwt').Strategy;
const ExtractJWT = require('passport-jwt').ExtractJwt;
const UserModel = require('./models/user');

passport.use(
  'signup',
  new LocalStrategy(
    {
      usernameField: 'username',
      passwordField: 'password',
    },
    async (username, password, done) => {
      try {
        const user = await UserModel.create({ username, password });
        return done(null, user);
      } catch (err) {
        return done(err);
      }
    }
  )
);

passport.use(
  'login',
  new LocalStrategy(
    {
      usernameField: 'username',
      passwordField: 'password',
    },
    async (username, password, done) => {
      try {
        const user = await UserModel.findOne({ username });
        if(!user) {
          return done(null, false, { message: 'User not found.' });
        }

        const validate = await user.isValidPassword(password);
        if(!validate) {
          return done(null, false, { message: 'Incorrect password.' });
        }

        return done(null, user);
      } catch(err) {
        return done(err);
      }
    }
  )
);

passport.use(
  new JwtStrategy(
    {
      secretOrKey: secret,
      jwtFromRequest: ExtractJWT.fromAuthHeaderAsBearerToken(),
    },
    async (token, done) => {
      try {
        const user = await UserModel.findOne({ username: token.user.username });
        if(!user) {
          return done(null, false, { message: 'User not found.' });
        }
        return done(null, token.user);
      } catch(err) {
        return done(err);
      }
    }
  )
);

//passport.serializeUser((user, done) => done(null, user.id));
//passport.deserializeUser(async (id, done) => {
//  return done(null, await getUserById(id));
//});

/*function initialize(passport, getUserByName, getUserById) {
  const authenticateUser = async (username, password, done) => {
    const user = await getUserByName(username);
    if (user === null) {
      return done(null, false, { message: 'User does not exist.' });
    }

    try {
      if (await bcrypt.compare(password, user.password)) {
        return done(null, user);
      } else {
        return done(null, false, { message: 'Password incorrect.' });
      }
    } catch (e) {
      return done(e);
    }
  };

  passport.use(new LocalStrategy({ usernameField: 'UserName' }, authenticateUser));
  passport.serializeUser((user, done) => done(null, user.id));
  passport.deserializeUser(async (id, done) => {
    return done(null, await getUserById(id));
  });
}

module.exports = {
  initialize,
};*/
