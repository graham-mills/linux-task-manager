import { useEffect, useCallback, useState } from "react";
import Card from "../UI/Card";
import { Config } from "../../config/config";
import MeterBar from "../UI/MeterBar";
import { Column, FlexTable, Label, Row, Value } from "../UI/FlexTable";
import { ConnectionStatus } from "../../context/conn-status";

const Processors = ({ connStatus }) => {
   const [cpuSnapshots, setCpuSnapshots] = useState([]);

   const fetchCpuSnapshots = useCallback(async () => {
      try {
         const response = await fetch(Config.Endpoints.Cpus);
         if (!response.ok) {
            return;
         }
         let data = await response.json();
         data.sort((a, b) => (a.id > b.id ? 1 : -1));
         setCpuSnapshots(data);
      } catch (error) {
         return;
      }
   }, []);

   const pollApi = useCallback(() => {
      if (connStatus === ConnectionStatus.Ok) {
         fetchCpuSnapshots();
      }
      setTimeout(pollApi, Config.POLL_PERIOD_MS);
   }, [connStatus, fetchCpuSnapshots]);

   useEffect(() => {
      pollApi();
   }, [pollApi]);

   const cpuRows = cpuSnapshots.map((cpu) => (
      <Row key={cpu.id}>
         <Label>{cpu.id}</Label>
         <Value>
            <MeterBar fillPercent={`${cpu.usage_percent.toFixed(0)}%`} />
         </Value>
      </Row>
   ));
   const midpoint = Math.ceil(cpuRows.length / 2);
   const cpuRowsColumn1 = cpuRows.slice(0, midpoint);
   const cpuRowsColumn2 = cpuRows.slice(midpoint, cpuRows.length);

   let totalUsage = 0;
   if (cpuSnapshots.length > 0) {
      const aggregateUsage = cpuSnapshots.reduce(
         (sum, cpu) => (sum += cpu.usage_percent),
         0
      );
      totalUsage = (aggregateUsage / (cpuSnapshots.length * 100)) * 100;
   }

   return (
      <Card title="Processors">
         <FlexTable columns={2}>
            <Column>
               <Row>
                  <Label>CPUs</Label>
                  <Value>{cpuSnapshots.length}</Value>
               </Row>
            </Column>
            <Column>
               <Row>
                  <Label>Utilization</Label>
                  <Value>{totalUsage.toFixed(2)}%</Value>
               </Row>
            </Column>
         </FlexTable>
         <FlexTable columns={2}>
            <Column>{cpuRowsColumn1}</Column>
            <Column>{cpuRowsColumn2}</Column>
         </FlexTable>
      </Card>
   );
};

export default Processors;
