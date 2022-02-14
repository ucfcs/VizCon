import React, { useRef, useState, useEffect } from 'react';
import '../styles/visualizer.scss'
function Visualizer(): React.ReactElement {
	const [msg, setMsg] = useState(null);
	useEffect(() => {
		let running = true;
		const task = async function() {
			// TODO:  cancellation is tricky?
			while (running) {
				const msg = await window.platform._temp_doStep();
				console.log("received visualizer state", msg);
				setMsg(msg);
				await delay(100);
			}
		}();
		return function cleanup() {
			running = false;
		}
	}, [/*todo?*/])
	if (msg === null)
		return <strong>Loading...</strong>
	return (<div>
		<h2>Visualizer!</h2>
		Threads:
		<ul>
			{msg.threads.map((thread: any) => {
				return <li key={thread.name}>{thread.active ? "*"+thread.name : thread.name}</li>
			})}
		</ul>
		<table className="globals">
			<caption>Globals</caption>
			<thead>
				<tr>
					<th>Type</th>
					<th>Name</th>
					<th>Value</th>
				</tr>
			</thead>
			<tbody>
			{msg.globals.map((global: any) => {
				return <tr key={global.name}><td>{global.type}</td><td>{global.name}</td><td>{global.value}</td></tr>
			})}
			</tbody>
		</table>
	</div>)
}

function delay(millis: number): Promise<void> {
	return new Promise((resolve, reject) => {
		setTimeout(
			() => { resolve(); },
			millis
		);
	});
}

export default Visualizer;