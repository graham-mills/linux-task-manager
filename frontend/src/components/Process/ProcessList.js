import Card from "../UI/Card";
import { useEffect, useState } from "react";
import { Config } from "../../config/config";
import { ConnectionStatus } from "../../context/conn-status";
import Button from "../UI/Button";
import { Column, FlexTable, Label, Row, Value } from "../UI/FlexTable";
import TreeTable from "./TreeTable";
import ListTable from "./ListTable";
import axios from "axios";

const fetchProcs = async () => {
   return axios
      .get(Config.Endpoints.Procs)
      .then((response) => response.data)
      .catch((error) => {
         console.error(error);
      });
};

const ProcessList = ({ connStatus }) => {
   const [displayAsTree, setDisplayAsTree] = useState(false);
   const [processes, setProcesses] = useState([]);

   useEffect(() => {
      if (connStatus !== ConnectionStatus.Ok) return;

      const interval = setInterval(() => {
         fetchProcs().then((data) => {
            setProcesses(data);
         });
      }, Config.POLL_PERIOD_MS);
      return () => {
         clearInterval(interval);
      };
   }, [connStatus]);

   return (
      <Card title="Processes">
         <FlexTable columns={2}>
            <Column>
               <Row>
                  <Label>Processes</Label>
                  <Value>{processes.length}</Value>
               </Row>
            </Column>
            <Column>
               <Row>
                  <Value>
                     <Button
                        selected={!displayAsTree}
                        onClick={() => {
                           setDisplayAsTree(false);
                        }}
                     >
                        List View
                     </Button>
                     <Button
                        selected={displayAsTree}
                        onClick={() => {
                           setDisplayAsTree(true);
                        }}
                     >
                        Tree View
                     </Button>
                  </Value>
               </Row>
            </Column>
         </FlexTable>

         {displayAsTree && <TreeTable processes={processes} />}
         {!displayAsTree && <ListTable processes={processes} />}
      </Card>
   );
};

export default ProcessList;
