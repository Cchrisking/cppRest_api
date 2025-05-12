import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    port: 3000,
    proxy: {
      '/api/books': {
        target: 'http://localhost:8080',
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/api\/books/, '/books')
      },
      '/api/users': {
        target: 'http://localhost:9080',
        changeOrigin: true,
        rewrite: (path) => path.replace(/^\/api\/users/, '/users')
      }
    }
  }
})