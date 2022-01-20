import { createApp } from 'vue';
import App from './App.vue';
import PrimeVue from 'primevue/config';
import './registerServiceWorker';
import router from './router';
import store from './store';
import 'primevue/resources/themes/lara-dark-indigo/theme.css';       //theme
import 'primevue/resources/primevue.min.css';                 //core css
import 'primeicons/primeicons.css';                           //icons
import 'primeflex/primeflex.css';                             //primeflex

const app = createApp(App);
app.use(store);
app.use(router);
app.use(PrimeVue);
app.mount('#app');
