import Card from "../UI/Card";
import { useCallback, useEffect, useState } from "react";
import { Config } from "../../config/config";
import { ConnectionStatus } from "../../context/conn-status";
import Button from "../UI/Button";
import { Column, FlexTable, Label, Row, Value } from "../UI/FlexTable";
import TreeTable from "./TreeTable";
import ListTable from "./ListTable";

const ProcessList = ({ connStatus }) => {
   const [displayAsTree, setDisplayAsTree] = useState(false);
   const [processes, setProcesses] = useState([]);

   const fetchProcs = useCallback(async () => {
      try {
         const response = await fetch(Config.Endpoints.Procs);
         if (!response.ok) {
            return;
         }
         const data = await response.json();
         setProcesses(data);
      } catch (error) {
         return;
      }
   }, []);

   const pollApi = useCallback(() => {
      if (connStatus === ConnectionStatus.Ok) {
         fetchProcs();
      }
      setTimeout(pollApi, Config.POLL_PERIOD_MS);
   }, [fetchProcs, connStatus]);

   useEffect(() => {
      pollApi();
   }, [pollApi]);

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
