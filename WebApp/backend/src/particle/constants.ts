import 'dotenv/config';

const username = process.env.PARTICLE_USERNAME ?? 'admin';
const password = process.env.PARTICLE_PASSWORD ?? 'admin';

export const particleConstants = {
  username,
  password,
};
