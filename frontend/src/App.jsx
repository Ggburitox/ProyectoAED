import { useState, useEffect, useRef } from 'react'
import Bucket from './components/Bucket'
import './App.css'

const API = "http://localhost:8080";

function App() {
  const [state, setState] = useState(null);
  const [val, setVal] = useState("");
  const logsEndRef = useRef(null); 

  const fetchState = async () => {
    try {
      const res = await fetch(`${API}/state`);
      if (!res.ok) throw new Error("Error");
      const data = await res.json();
      setState(data);
    } catch(e) { console.error("Conectando..."); }
  };

  useEffect(() => { fetchState(); }, []);

  useEffect(() => {
    logsEndRef.current?.scrollIntoView({ behavior: "smooth" });
  }, [state?.logs]);

  const handleAction = async (endpoint) => {
    if(!val) return;
    await fetch(`${API}/${endpoint}`, {
      method: 'POST',
      body: JSON.stringify({ key: parseInt(val) })
    });
    setVal("");
    fetchState();
  };

  const handleReset = async () => {
    await fetch(`${API}/reset`, { method: 'POST' });
    fetchState();
  };

  if (!state) return <div className="loading">Cargando Sistema...</div>;

  const getOverflow = (idx) => state.overflow && state.overflow[String(idx)] ? state.overflow[String(idx)] : [];

  return (
    <div className="main-layout">
      
      {/* 1. CUADRO IZQUIERDO: HISTORIAL */}
      <div className="logs-panel">
        <h3>Historial de Operaciones</h3>
        <div className="logs-list">
          {state.logs.length === 0 && <div className="log-empty">Esperando acciones...</div>}
          {state.logs.map((l, i) => (
            <div key={i} className="log-item">
              <span className="log-icon">➤</span> {l}
            </div>
          ))}
          <div ref={logsEndRef} />
        </div>
      </div>

      {/* 2. AREA DERECHA: VISUALIZADOR */}
      <div className="viz-panel">
        <h1>Linear Hashing Visualizer</h1>
        
        <div className="stats-header">
          <div className="stat-box">Nivel (i): <span>{state.level}</span></div>
          <div className="stat-box">Buckets (N): <span>{state.N}</span></div>
          <div className="stat-box">Puntero (p): <span>{state.p}</span></div>
        </div>

        <div className="controls-area">
          <input 
            type="number" 
            value={val} 
            onChange={e => setVal(e.target.value)} 
            placeholder="Valor..."
            onKeyDown={(e) => e.key === 'Enter' && handleAction('insert')}
          />
          <button className="btn-ins" onClick={() => handleAction('insert')}>Insertar</button>
          <button className="btn-del" onClick={() => handleAction('delete')}>Eliminar</button>
          <button className="btn-rst" onClick={handleReset}>Reset</button>
        </div>

        <div className="buckets-grid">
          {state.buckets.map((vals, idx) => (
            <Bucket 
              key={idx} 
              index={idx} 
              values={vals} 
              isP={idx === state.p} 
              overflow={getOverflow(idx)}
            />
          ))}
        </div>
      </div>

    </div>
  )
}

export default App