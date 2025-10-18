export default {
  server: {
    allowedHosts: true,
    proxy: {
      '/api': {
        target: 'http://localhost:8000/',
        changeOrigin: true
      }
    }
  },
};
