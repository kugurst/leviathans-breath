import Vue from 'vue';
import axios from 'axios';

import App from './App';
import router from './router';
import store from './store';

const LeviathansBreathDriver = require('leviathans_breath_driver');

if (!process.env.IS_WEB) Vue.use(require('vue-electron'));
Vue.http = Vue.prototype.$http = axios;
Vue.config.productionTip = false;

if (!LeviathansBreathDriver.Connect()) {
  App.quit();
}

/* eslint-disable no-new */
new Vue({
  components: { App },
  router,
  store,
  template: '<App/>',
}).$mount('#app');

console.log(LeviathansBreathDriver.GetAllTemperatures());
