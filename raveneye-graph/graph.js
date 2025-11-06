// graph.js - 3D force graph viewer
const Graph = ForceGraph3D();

let rawData = null;
let graph = null;
const el = document.getElementById('graph');

function setStatus(s){ document.getElementById('status').textContent = 'Status: ' + s; }

graph = Graph(el)
    .graphData({ nodes: [], links: [] })
    .nodeAutoColorBy('group')
    .nodeThreeObject(node => {
        const group = new THREE.Group();
        const size = Math.max(1, (node.size || 1));
        const sphere = new THREE.Mesh(
            new THREE.SphereGeometry(Math.cbrt(size) * 2),
            new THREE.MeshStandardMaterial({ color: node.color || undefined, metalness: 0.1, roughness: 0.8 })
        );
        group.add(sphere);

        // simple label sprite
        const sprite = makeTextSprite(node.label || node.name || node.id);
        sprite.position.set((Math.cbrt(size) * 2.4), 0, 0);
        sprite.scale.set(0.1, 0.1, 0.1);
        group.add(sprite);

        return group;
    })
    .linkWidth(l => Math.max(0.5, (l.weight||1)/2))
    .linkDirectionalParticles(0) // no particles by default
    .onNodeClick(node => {
        const distance = 40;
        const distRatio = 1 + distance/Math.hypot(node.x||0, node.y||0, node.z||0);
        graph.cameraPosition(
            { x: (node.x||0)*distRatio, y: (node.y||0)*distRatio, z: (node.z||0)*distRatio },
            { x: node.x||0, y: node.y||0, z: node.z||0 },
            800
        );
        setStatus('Clicked node: ' + (node.label||node.id));
        // If bridge exists (Qt embed), call it: window.bridge.nodeClicked(node.id)
        if (window.bridge && window.bridge.nodeClicked) try{ window.bridge.nodeClicked(node.id); }catch(e){}
    })
    .onNodeHover(node => document.body.style.cursor = node ? 'pointer' : 'default')
    .enableNodeDrag(true)
    .backgroundColor('#0b0b0f')
;

// lighting
const threeScene = graph.scene();
threeScene.add(new THREE.AmbientLight(0x888888));
const directional = new THREE.DirectionalLight(0xffffff, 0.6);
directional.position.set(0.5, 1, 0.5);
threeScene.add(directional);

// Text sprite util
function makeTextSprite(message){
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    const fontSize = 48;
    ctx.font = `${fontSize}px Arial`;
    const w = Math.ceil(ctx.measureText(message).width) + 20;
    canvas.width = w;
    canvas.height = fontSize + 30;
    ctx.font = `${fontSize}px Arial`;
    ctx.fillStyle = 'white';
    ctx.fillText(message, 10, fontSize);
    const texture = new THREE.CanvasTexture(canvas);
    const sprite = new THREE.Sprite(new THREE.SpriteMaterial({ map: texture, transparent: true }));
    return sprite;
}

// load static data.json initially
fetch('data.json').then(r=>r.json()).then(d=>{
    rawData = d;
    processAndRender(d);
    setStatus('Loaded data.json');
}).catch(e=>setStatus('No data.json (okay)'));

// UI hooks
document.getElementById('fit').addEventListener('click', ()=>graph.zoomToFit(400));
document.getElementById('minWeight').addEventListener('input', ()=>applyFilter());
document.getElementById('search').addEventListener('change', ()=>{
    const q = document.getElementById('search').value.trim().toLowerCase();
    if (!q || !rawData) return;
    const node = rawData.nodes.find(n => (''+n.id)===q || (n.name||'').toLowerCase().includes(q));
    if (node) { graph.centerAt(node.x||0, node.y||0, 1000); graph.zoom(4, 1000); setStatus('Found: ' + (node.name||node.id)); }
});

function processAndRender(d){
    const degrees = {};
    (d.links||[]).forEach(l => { degrees[l.source] = (degrees[l.source]||0)+1; degrees[l.target] = (degrees[l.target]||0)+1; });
    d.nodes.forEach(n => {
        n.size = Math.max(1, degrees[n.id] || 1);
        n.label = n.name || (''+n.id);
        n.group = n.tag || 'unknown';
    });
    graph.graphData(d);
    graph.zoomToFit(0);
}

// exposed function for external updates (e.g. via websocket or Qt)
window.updateGraph = function(jsonObj){
    rawData = jsonObj;
    processAndRender(jsonObj);
    setStatus('Graph updated');
};

// filtering
function applyFilter(){
    const minW = +document.getElementById('minWeight').value;
    if (!rawData) return;
    const links = (rawData.links||[]).filter(l => (l.weight||0) >= minW);
    const used = new Set();
    links.forEach(l => { used.add(l.source); used.add(l.target); });
    const nodes = (rawData.nodes||[]).filter(n => used.has(n.id));
    graph.graphData({ nodes, links });
}

// If using the Node server with WebSocket updates, the server will push JSON via ws and call window.updateGraph
// (see server.js for WebSocket message format)
