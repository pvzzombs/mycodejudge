export default {
  server: {
    port: 3000,
    allowedHosts: true,
    proxy: {
      '/api': {
        target: 'http://localhost:8080/',
        changeOrigin: true
      }
    }
  },
};
