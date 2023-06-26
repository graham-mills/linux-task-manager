import { useEffect, useCallback, useState } from "react";
import Card from "../UI/Card";
import { Config } from "../../config/config";
import MeterBar from "../UI/MeterBar";
import { Column, FlexTable, Label, Row, Value } from "../UI/FlexTable";
import { ConnectionStatus } from "../../context/conn-status";

const Memory = ({ connStatus }) => {
   const [memTotal, setMemTotal] = useState(0);
   const [memFree, setMemFree] = useState(0);
   const [memUsage, setMemUsage] = useState(0);

   const fetchMemorySnapshot = async () => {
      try {
         const response = await fetch(Config.Endpoints.Mem);
         if (!response.ok) {
            return;
         }
         const data = await response.json();
         setMemTotal(data.total_memory_kB);
         setMemFree(data.free_memory_kB);
         setMemUsage(data.usage_percent.toFixed(0));
      } catch (error) {
         return;
      }
   };

   const pollApi = useCallback(() => {
      if (connStatus === ConnectionStatus.Ok) {
         fetchMemorySnapshot();
      }
      setTimeout(pollApi, Config.POLL_PERIOD_MS);
   }, [connStatus]);

   useEffect(() => {
      pollApi();
   }, [pollApi]);

   const memTotalMB = (memTotal / 1024).toFixed(0);
   const memTotalGB = (memTotalMB / 1024).toFixed(2);
   const memFreeMB = (memFree / 1024).toFixed(0);
   const memFreeGB = (memFreeMB / 1024).toFixed(2); 
   const memUsedMB = memTotalMB - memFreeMB;
   const memUsedGB = (memTotalGB - memFreeGB).toFixed(2);

   return (
      <Card title="Memory">
         <MeterBar fillPercent={`${memUsage}%`} />
         <FlexTable columns={3}>
            <Column>
               <Row>
                  <Label>Total</Label>
                  <Value>{memTotalMB} MB</Value>
               </Row>
               <Row>
                  <Label></Label>
                  <Value>{memTotalGB} GB</Value>
               </Row>
            </Column>
            <Column>
               <Row>
                  <Label>Free</Label>
                  <Value>{memFreeMB} MB</Value>
               </Row>
               <Row>
                  <Label></Label>
                  <Value>{memFreeGB} GB</Value>
               </Row>
            </Column>
            <Column>
               <Row>
                  <Label>Used</Label>
                  <Value>{memUsedMB} MB</Value>
               </Row>
               <Row>
                  <Label></Label>
                  <Value>{memUsedGB} GB</Value>
               </Row>
            </Column>
         </FlexTable>
      </Card>
   );
};

export default Memory;
