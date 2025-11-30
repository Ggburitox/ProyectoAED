import React from 'react';
import './Bucket.css';

const Bucket = ({ index, values, isP, overflow }) => {
  const capacity = 4; 
  const safeOverflow = overflow || [];

  return (
    <div className="bucket-wrapper-row">
      
      <div className="p-container">
        {isP ? <span className="p-arrow-left">p ➡</span> : null}
      </div>

      <div className="bucket-column">
        {/* Bucket Principal */}
        <div className={`bucket-box ${isP ? 'highlight-p' : ''}`}>
          <div className="bucket-idx">Bucket {index}</div>
          <div className="slots-container">
            {Array.from({ length: capacity }).map((_, i) => (
              <div key={i} className={`slot ${values[i] !== undefined ? 'filled' : ''}`}>
                {values[i] ?? ""}
              </div>
            ))}
          </div>
        </div>

        {/* Overflow con Flecha */}
        {safeOverflow.length > 0 && (
          <div className="overflow-wrapper">
            {/* Dibujo de la flecha de conexión */}
            <div className="connector-line"></div>
            <div className="connector-arrow"></div>
            
            <div className="overflow-box">
              {safeOverflow.map((v, k) => (
                <div key={k} className="slot overflow-item">{v}</div>
              ))}
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default Bucket;