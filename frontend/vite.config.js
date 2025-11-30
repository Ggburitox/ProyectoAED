import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [react()],
  server: {
    host: true,       // <--- ESTO ES LA CLAVE MÁGICA
    strictPort: true,
    port: 5173,
    watch: {
      usePolling: true // <--- Esto ayuda en Windows si no recarga
    }
  }
})