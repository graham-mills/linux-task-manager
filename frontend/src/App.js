import { Fragment, useState } from 'react';
import Overview from './components/Overview/Overview';
import ProcessList from './components/Process/ProcessList';
import Processors from './components/Processors/Processors';
import Memory from './components/Memory/Memory';
import { ConnectionStatus } from './context/conn-status';

const App = () => {
  const [connStatus, setConnStatus] = useState(ConnectionStatus.Connecting);
  
  return (
    <Fragment>
      <Overview connStatus={connStatus} setConnStatus={setConnStatus} />
      <Memory connStatus={connStatus}/>
      <Processors connStatus={connStatus}/>
      <ProcessList connStatus={connStatus}/>
    </Fragment>
  );
}

export default App;
