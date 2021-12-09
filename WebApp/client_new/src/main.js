import { createApp } from 'vue';
// import { BootstrapVue, IconsPlugin } from 'bootstrap-vue';
import App from './App.vue';
import router from './router';

let app = createApp(App);
app.use(router);
// app.use(BootstrapVue);
// app.use(IconsPlugin);

app.mount('#app');
