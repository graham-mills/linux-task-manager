import { useEffect, useState } from "react";
import { Config } from "../../config/config";
import Card from "../UI/Card";
import MeterBar from "../UI/MeterBar";
import { Column, FlexTable, Label, Row, Value } from "../UI/FlexTable";
import { ConnectionStatus } from "../../context/conn-status";
import axios from "axios";

const fetchMemorySnapshot = async () => {
   return axios
      .get(Config.Endpoints.Mem)
      .then((response) => response.data)
      .catch((error) => {
         console.log(error);
      });
};

const Memory = ({ connStatus }) => {
   const [memSnapshot, setMemSnapshot] = useState({
      total_memory_kB: 0,
      free_memory_kB: 0,
      usage_percent: 0,
   });

   useEffect(() => {
      if (connStatus !== ConnectionStatus.Ok) {
         return;
      }
      const interval = setInterval(() => {
         fetchMemorySnapshot().then((data) => {
            if (data) {
               setMemSnapshot(data);
            }
         });
      }, Config.POLL_PERIOD_MS);
      return () => {
         clearInterval(interval);
      };
   }, [connStatus, setMemSnapshot]);

   const memTotalMB = (memSnapshot.total_memory_kB / 1024).toFixed(0);
   const memTotalGB = (memTotalMB / 1024).toFixed(2);
   const memFreeMB = (memSnapshot.free_memory_kB / 1024).toFixed(0);
   const memFreeGB = (memFreeMB / 1024).toFixed(2);
   const memUsedMB = memTotalMB - memFreeMB;
   const memUsedGB = (memTotalGB - memFreeGB).toFixed(2);

   return (
      <Card title="Memory">
         <MeterBar fillPercent={`${memSnapshot.usage_percent.toFixed(0)}%`} />
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
