import { createApp } from 'vue';
import App from './App.vue';
import PrimeVue from 'primevue/config';
import ToastService from 'primevue/toastservice';
import './registerServiceWorker';
import router from './router';
import store from './store';
import 'primevue/resources/themes/lara-dark-indigo/theme.css';       //theme
import 'primevue/resources/primevue.min.css';                 //core css
import 'primeicons/primeicons.css';                           //icons
import 'primeflex/primeflex.css';                             //primeflex
import Tooltip from 'primevue/tooltip';

const app = createApp(App);
app.use(store);
app.use(router);
app.use(PrimeVue);
app.use(ToastService);
app.directive('tooltip', Tooltip);
app.mount('#app');
