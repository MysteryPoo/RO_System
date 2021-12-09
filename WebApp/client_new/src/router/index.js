import { createWebHistory, createRouter } from "vue-router";
import Home from '@/views/Home.vue';
import Configuration from '@/views/Configuration.vue';
import Login from '@/views/Login.vue';

const routes = [
  {
    path: '/',
    name: 'Home',
    component: Home,
  },
  {
    path: '/about',
    name: 'About',
    // route level code-splitting
    // this generates a separate chunk (about.[hash].js) for this route
    // which is lazy-loaded when the route is visited.
    component() {
      return import(/* webpackChunkName: "about" */ '../views/About.vue');
    },
  },
  {
    path: '/config',
    name: 'Configuration',
    component: Configuration,
    props: true,
  },
  {
    path: '/login',
    name: 'Login',
    component: Login,
    props: true,
  },
];

const router = createRouter({
  history: createWebHistory(),
  routes,
});

export default router;
