import { createStore } from 'vuex'

export default createStore({
  state: {
    token: window.localStorage.token,
  },
  mutations: {
    set_token(state, _token) {
      state.token = _token;
    }
  },
  actions: {
  },
  modules: {
  }
})
