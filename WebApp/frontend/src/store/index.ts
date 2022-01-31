import { createStore } from 'vuex'

export default createStore({
  state: {
    token: window.localStorage.token,
    version: __APP_VERSION__,
  },
  mutations: {
    set_token(state, _token) {
      state.token = _token;
    }
  },
  actions: {
  },
  modules: {
  },
})
