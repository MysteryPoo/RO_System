import { RouteRecordRaw } from 'vue-router';

const routes: RouteRecordRaw[] = [
  {
    path: '/',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '', component: () => import('pages/IndexPage.vue') },
      { path: '/status', component: () => import('pages/IndexPage.vue')},
      { path: '/logs', component: () => import('pages/LogPage.vue')},
      { path: '/configuration', component: () => import('pages/ConfigurationPage.vue')},
      { path: '/test', component: () => import('pages/TestPage.vue')},
      { path: '/auth', component: () => import('pages/LoginPage.vue')},
      { path: '/auth/out', component: () => import('pages/LogoutPage.vue')},
    ],
  },

  // Always leave this as last one,
  // but you can also remove it
  {
    path: '/:catchAll(.*)*',
    component: () => import('layouts/MainLayout.vue'),
    children: [
      { path: '', component: () => import('pages/ErrorNotFound.vue')},
    ]
  },
];

export default routes;
