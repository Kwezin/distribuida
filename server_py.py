# server_py.py
from concurrent import futures
import grpc
import notas_pb2
import notas_pb2_grpc

# banco em memória: chave ra_cod
db = {}

class GerenciadorNotasServicer(notas_pb2_grpc.GerenciadorNotasServicer):
    def AdicionarNota(self, request, context):
        chave = f"{request.ra}_{request.cod_disciplina}"
        if chave in db:
            return notas_pb2.StatusResponse(sucesso=False, msg="Nota já existe")
        nota = notas_pb2.Nota(
            ra=request.ra,
            cod_disciplina=request.cod_disciplina,
            ano=request.ano,
            semestre=request.semestre,
            nota=request.nota
        )
        db[chave] = nota
        return notas_pb2.StatusResponse(sucesso=True, msg="Nota adicionada")

    def AlterarNota(self, request, context):
        chave = f"{request.ra}_{request.cod_disciplina}"
        if chave not in db:
            return notas_pb2.StatusResponse(sucesso=False, msg="Nota não existe")
        nota = db[chave]
        nota.ano = request.ano
        nota.semestre = request.semestre
        nota.nota = request.nota
        db[chave] = nota
        return notas_pb2.StatusResponse(sucesso=True, msg="Nota alterada")

    def ConsultarNota(self, request, context):
        chave = f"{request.ra}_{request.cod_disciplina}"
        if chave not in db:
            return notas_pb2.ConsultaNotaResponse(sucesso=False, msg_erro="Nota não encontrada")
        return notas_pb2.ConsultaNotaResponse(sucesso=True, nota=db[chave])

    def CalcularMedia(self, request, context):
        ra = request.ra
        notas = [v.nota for k,v in db.items() if v.ra == ra]
        if not notas:
            return notas_pb2.MediaResponse(sucesso=False, media=0.0, msg_erro="Aluno sem notas")
        media = sum(notas) / len(notas)
        return notas_pb2.MediaResponse(sucesso=True, media=media)

    def ListarNotasAluno(self, request, context):
        ra = request.ra
        for k,v in db.items():
            if v.ra == ra:
                yield v

def serve():
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=10))
    notas_pb2_grpc.add_GerenciadorNotasServicer_to_server(GerenciadorNotasServicer(), server)
    server.add_insecure_port('[::]:50052')
    server.start()
    print("Servidor Python escutando em 0.0.0.0:50052")
    server.wait_for_termination()

if __name__ == '__main__':
    serve()
